// ======================================================================
// \title  PrmDb.cpp
// \author laboratory10
// \brief  cpp file for PrmDb component implementation class
// ======================================================================

#include <Components/PrmDb/PrmDb.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <cstring>
#include <cstdio>
#include <SD.h>

namespace Components {


  typedef PrmDb_PrmWriteError PrmWriteError;
  typedef PrmDb_PrmReadError PrmReadError;
  // anonymous namespace for buffer declaration
  namespace {
    class WorkingBuffer : public Fw::SerializeBufferBase {
      public:

        NATIVE_UINT_TYPE getBuffCapacity() const {
          return sizeof(m_buff);
        }

        U8* getBuffAddr() {
          return m_buff;
        }

        const U8* getBuffAddr() const {
          return m_buff;
        }

      private:
        // Set to max of parameter buffer + id
        U8 m_buff[FW_PARAM_BUFFER_MAX_SIZE + sizeof(FwPrmIdType)];
    };
  }

  PrmDb::PrmDb(const char* name) : PrmDbComponentBase(name) {
    this->clearDb();
  }

  void PrmDb::configure(const char* file) {
    FW_ASSERT(file != nullptr);
    this->m_fileName = file;
  }

  void PrmDb::clearDb() {
    for (I32 entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
      this->m_db[entry].used = false;
      this->m_db[entry].id = 0;
    }
  }

  // If ports are no longer guarded, these accesses need to be protected
  // If there are a lot of accesses an interrupt lock could be used instead 

  Fw::ParamValid PrmDb::getPrm_handler(
    NATIVE_INT_TYPE portNum, 
    FwPrmIdType id, 
    Fw::ParamBuffer &val) 
  {
    // search for entry
    Fw::ParamValid stat = Fw::ParamValid::INVALID;

    for (I32 entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
      if (this->m_db[entry].used) {
        if (this->m_db[entry].id == id) {
          val = this->m_db[entry].val;
          stat = Fw::ParamValid::VALID;
          break;
        }
      }
    }

    // if unable to find parameter, send error message
    if (Fw::ParamValid::INVALID == stat.e) {
      this->log_WARNING_LO_PrmIdNotFound(id);
    }

    return stat;
  }

  void PrmDb::setPrm_handler(
    NATIVE_INT_TYPE portNum, 
    FwPrmIdType id, 
    Fw::ParamBuffer &val) 
  {

    this->lock();

    // search for existing entry

    bool existingEntry = false;
    bool noSlots = true;

    for (NATIVE_INT_TYPE entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
      if ((this->m_db[entry].used) && (id == this->m_db[entry].id)) {
        this->m_db[entry].val = val;
        existingEntry = true;
        break;
      }
    }

    // if there is no existing entry, add one
    if (!existingEntry) {
      for (I32 entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        if (!(this->m_db[entry].used)) {
          this->m_db[entry].val = val;
          this->m_db[entry].id = id;
          this->m_db[entry].used = true;
          noSlots = false;
          break;
        }
      }
    }

    this->unLock();

    if (existingEntry) {
      this->log_ACTIVITY_HI_PrmIdUpdated(id);
    } else if (noSlots) {
      this->log_FATAL_PrmDbFull(id);
    } else {
      this->log_ACTIVITY_HI_PrmIdAdded(id);
    }

  }

  void PrmDb::PRM_SAVE_FILE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
      
    FW_ASSERT(this->m_fileName.length() > 0);
    SDFile param_file;
    size_t stat;
    WorkingBuffer buff;
    SD.remove(this->m_fileName.toChar());
    param_file = SD.open(this->m_fileName.toChar(), FILE_WRITE);
    if (!param_file) {
      this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::OPEN,0,param_file);
      this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::EXECUTION_ERROR);
      return;
    }

    this->lock();

    // Traverse the parameter list, saving each entry

    U32 numRecords = 0;

    for (
          NATIVE_UINT_TYPE entry = 0; 
          entry < FW_NUM_ARRAY_ELEMENTS(this->m_db); 
          entry++
        ) 
    {
      if (this->m_db[entry].used) {
        // write delimiter
        static const U8 delim = PRMDB_ENTRY_DELIMITER;
        NATIVE_INT_TYPE writeSize = sizeof(delim);
        stat = param_file.write(&delim,writeSize);
        if (writeSize != stat) {
          this->unLock();
          this->log_WARNING_HI_PrmFileWriteError(
            PrmWriteError::DELIMITER_SIZE,
            numRecords,
            stat);
          this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::EXECUTION_ERROR);
          return;
        }
        // serialize record size = id field + data
        U32 recordSize = (sizeof(FwPrmIdType) + 
                          this->m_db[entry].val.getBuffLength());

        // reset buffer
        buff.resetSer();
        Fw::SerializeStatus serStat = buff.serialize(recordSize);
        // should always work
        FW_ASSERT(Fw::FW_SERIALIZE_OK == 
          serStat,static_cast<NATIVE_INT_TYPE>(serStat));

        // write record size
        writeSize = buff.getBuffLength();
        stat = param_file.write(buff.getBuffAddr(),writeSize);
        if (writeSize != stat) {
          this->unLock();
          this->log_WARNING_HI_PrmFileWriteError(
            PrmWriteError::RECORD_SIZE_SIZE,
            numRecords,
            stat);
          this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::EXECUTION_ERROR);
          return;
        }

        // reset buffer
        buff.resetSer();

        // serialize parameter id

        serStat = buff.serialize(this->m_db[entry].id);
        // should always work
        FW_ASSERT(Fw::FW_SERIALIZE_OK == 
          serStat,static_cast<NATIVE_INT_TYPE>(serStat));

        // write parameter ID
        writeSize = buff.getBuffLength();
        stat = param_file.write(buff.getBuffAddr(),writeSize);
        if (writeSize != stat) {
          this->unLock();
          this->log_WARNING_HI_PrmFileWriteError(
            PrmWriteError::PARAMETER_ID_SIZE,
            numRecords,
            stat);
          this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::EXECUTION_ERROR);
          return;
        }

        // write serialized parameter value

        writeSize = this->m_db[entry].val.getBuffLength();
        stat = param_file.write(this->m_db[entry].val.getBuffAddr(),writeSize);
        if (writeSize != stat) {
          this->unLock();
          this->log_WARNING_HI_PrmFileWriteError(
            PrmWriteError::PARAMETER_VALUE_SIZE,
            numRecords,
            stat);
          this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::EXECUTION_ERROR);
          return;
        }
        numRecords++;
      } // end if record in use
    } // end for each record

    param_file.close();
    this->unLock();
    this->log_ACTIVITY_HI_PrmFileSaveComplete(numRecords);
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);

  }

  PrmDb::~PrmDb() {
  }

  void PrmDb::readParamFile() {
    FW_ASSERT(this->m_fileName.length() > 0);
    // load file. FIXME: Put more robust file checking, such as a CRC.
    SDFile param_file;
    size_t stat;

    param_file = SD.open(this->m_fileName.toChar(), FILE_READ);
    if (!param_file) {
      this->log_WARNING_HI_PrmFileReadError(PrmReadError::OPEN,0,param_file);
      return;
    }

    WorkingBuffer buff;

    U32 recordNum = 0;

    this->clearDb();

    for (NATIVE_INT_TYPE entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++)  {

      U8 delimiter;
      NATIVE_INT_TYPE readSize = sizeof(delimiter);

      // read delimiter
      stat = param_file.read(&delimiter,readSize);
      // check for end of file (read size 0)
      if (0 == stat) {
        break;
      }

      if (readSize != stat) {
        this->log_WARNING_HI_PrmFileReadError(
          PrmReadError::DELIMITER_SIZE,
          recordNum,
          stat);
        return;
      }

      if (PRMDB_ENTRY_DELIMITER != delimiter) {
        this->log_WARNING_HI_PrmFileReadError(
          PrmReadError::DELIMITER_VALUE,
          recordNum,
          delimiter);
        return;
      }

      U32 recordSize = 0;
      // read record size
      readSize = sizeof(recordSize);

      stat = param_file.read(buff.getBuffAddr(),readSize);
      if (readSize != stat) {
        this->log_WARNING_HI_PrmFileReadError(
          PrmReadError::RECORD_SIZE_SIZE,
          recordNum,
          stat);
        return;
      }
      // set serialized size to read size
      Fw::SerializeStatus desStat = buff.setBuffLen(readSize);
      // should never fail
      FW_ASSERT(Fw::FW_SERIALIZE_OK == 
        desStat,static_cast<NATIVE_INT_TYPE>(desStat));
      // reset deserialization
      buff.resetDeser();
      // deserialize, since record size is serialized in file
      desStat = buff.deserialize(recordSize);
      FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat);

      // sanity check value
      // It can't be larger than the maximum parameter buffer size + id
      // or smaller than the record id
      if ((recordSize > FW_PARAM_BUFFER_MAX_SIZE + sizeof(U32)) or 
        (recordSize < sizeof(U32))) 
      {
        this->log_WARNING_HI_PrmFileReadError(
          PrmReadError::RECORD_SIZE_VALUE,
          recordNum,
          recordSize);
        return;
      }

      // read the parameter ID
      FwPrmIdType parameterId = 0;
      readSize = sizeof(FwPrmIdType);

      stat = param_file.read(buff.getBuffAddr(),readSize);
      if (readSize != stat) {
        this->log_WARNING_HI_PrmFileReadError(
          PrmReadError::PARAMETER_ID_SIZE,
          recordNum,
          stat);
        return;
      }

      // set serialized size to read parameter ID
      desStat = buff.setBuffLen(readSize);
      // should never fail
      FW_ASSERT(Fw::FW_SERIALIZE_OK == 
        desStat,static_cast<NATIVE_INT_TYPE>(desStat));
      // reset deserialization
      buff.resetDeser();
      // deserialize, since parameter ID is serialized in file
      desStat = buff.deserialize(parameterId);
      FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat);

      // copy parameter
      this->m_db[entry].used = true;
      this->m_db[entry].id = parameterId;
      readSize = recordSize-sizeof(parameterId);

      stat = param_file.read(this->m_db[entry].val.getBuffAddr(),readSize);

      if (readSize != stat) {
        this->log_WARNING_HI_PrmFileReadError(
          PrmReadError::PARAMETER_VALUE_SIZE,
          recordNum,
          stat);
        return;
      }

      // set serialized size to read size
      desStat = this->m_db[entry].val.setBuffLen(readSize);
      // should never fail
      FW_ASSERT(Fw::FW_SERIALIZE_OK == 
        desStat,static_cast<NATIVE_INT_TYPE>(desStat));
      recordNum++;

    }

    param_file.close();
    this->log_ACTIVITY_HI_PrmFileLoadComplete(recordNum);
  }

}
