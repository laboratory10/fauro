module Fauro {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

    enum Ports_RateGroups {
      rateGroup5sec
      rateGroup6Hz
    }

    enum Ports_StaticMemory {
      framer
      deframer
      deframing
    }

  topology Fauro {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance commandDispatcher
    instance telemetryManager
    instance deframer
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance framer
    instance rateDriver
    instance rateGroup5sec
    instance rateGroup6Hz
    instance rateGroupDriver
    instance staticMemory
    instance textLogger
    instance tlmSend
    instance fswManager
    instance fauroTime
    instance gncManager
    instance powerManager
    instance prmDb

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance commandDispatcher

    event connections instance eventLogger

    param connections instance prmDb

    telemetry connections instance tlmSend

    text event connections instance textLogger

    time connections instance fauroTime

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {
      # Block driver
      rateDriver.CycleOut -> rateGroupDriver.CycleIn

      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup5sec] -> rateGroup5sec.CycleIn
      rateGroup5sec.RateGroupMemberOut[0] -> fswManager.schedIn
      rateGroup5sec.RateGroupMemberOut[1] -> powerManager.schedIn

      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup6Hz] -> rateGroup6Hz.CycleIn
      rateGroup6Hz.RateGroupMemberOut[0] -> telemetryManager.schedIn
      rateGroup6Hz.RateGroupMemberOut[1] -> tlmSend.Run
      rateGroup6Hz.RateGroupMemberOut[2] -> gncManager.gncUpdate

    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections Downlink {

      tlmSend.PktSend -> framer.comIn
      eventLogger.PktSend -> framer.comIn

      framer.framedAllocate -> staticMemory.bufferAllocate[Ports_StaticMemory.framer]
      framer.framedOut -> telemetryManager.$send

      telemetryManager.deallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.framer]

    }
    
    connections Uplink {

      telemetryManager.allocate -> staticMemory.bufferAllocate[Ports_StaticMemory.deframer]
      telemetryManager.$recv -> deframer.framedIn
      deframer.framedDeallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.deframer]

      deframer.comOut -> commandDispatcher.seqCmdBuff
      commandDispatcher.seqCmdStatus -> deframer.cmdResponseIn

      deframer.bufferAllocate -> staticMemory.bufferAllocate[Ports_StaticMemory.deframing]
      deframer.bufferDeallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.deframing]
      
    }

    connections Fauro {
      # Add here connections to user-defined components
    }

  }

}
