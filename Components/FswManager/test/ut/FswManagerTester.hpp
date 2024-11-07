// ======================================================================
// \title  FswManagerTester.hpp
// \author laboratory10
// \brief  hpp file for FswManager component test harness implementation class
// ======================================================================

#ifndef Components_FswManagerTester_HPP
#define Components_FswManagerTester_HPP

#include "Components/FswManager/FswManagerGTestBase.hpp"
#include "Components/FswManager/FswManager.hpp"

namespace Components {

  class FswManagerTester :
    public FswManagerGTestBase
  {

    public:

      // ----------------------------------------------------------------------
      // Constants
      // ----------------------------------------------------------------------

      // Maximum size of histories storing events, telemetry, and port outputs
      static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;

      // Instance ID supplied to the component instance under test
      static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object FswManagerTester
      FswManagerTester();

      //! Destroy object FswManagerTester
      ~FswManagerTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      void toDo();

    private:

      // ----------------------------------------------------------------------
      // Helper functions
      // ----------------------------------------------------------------------

      //! Connect ports
      void connectPorts();

      //! Initialize components
      void initComponents();

      void setupTestMemory();

    private:

      // ----------------------------------------------------------------------
      // Member variables
      // ----------------------------------------------------------------------

      //! The component under test
      FswManager component;

  };

}

#endif
