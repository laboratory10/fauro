// ======================================================================
// \title  FauroTopology.cpp
// \brief cpp file containing the topology instantiation code
//
// ======================================================================
// Provides access to autocoded functions
#include <Fauro/Top/FauroTopologyAc.hpp>
#include <Fauro/Top/FauroPacketsAc.hpp>
#include <config/FppConstantsAc.hpp>

// Necessary project-specified types
#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/FramingProtocol/FprimeProtocol.hpp>

// Allows easy reference to objects in FPP/autocoder required namespaces
using namespace Fauro;

// The reference topology uses a malloc-based allocator for components that need to allocate memory during the
// initialization phase.
Fw::MallocAllocator mallocator;

// The reference topology uses the F´ packet protocol when communicating with the ground and therefore uses the F´
// framing and deframing implementations.
Svc::FprimeFraming framing;
Svc::FprimeDeframing deframing;

// The reference topology divides the incoming clock signal (1Hz) into sub-signals: 1/100Hz, 1/200Hz, and 1/1000Hz

// The comment above is automatically generated and left in for the record, but its wrong in our case
// The fprime-arduino HardwareRateDriver (the instance of which is rateDriver) is configured below
// with a 1ms tick. So a "1" here means 1000Hz and a "1000" means 1 hertz.
// So here we have a 1hz, at 10Hz, and a 0.2Hz rate group
Svc::RateGroupDriver::DividerSet rateGroupDivisors{{{5000, 0}, {100, 0}, {0, 0}}};

// Rate groups may supply a context token to each of the attached children whose purpose is set by the project. The
// reference topology sets each token to zero as these contexts are unused in this project.
NATIVE_INT_TYPE rateGroup5secContext[FppConstant_PassiveRateGroupOutputPorts::PassiveRateGroupOutputPorts] = {};
NATIVE_INT_TYPE rateGroup6HzContext[FppConstant_PassiveRateGroupOutputPorts::PassiveRateGroupOutputPorts] = {};

/**
 * \brief configure/setup components in project-specific way
 *
 * This is a *helper* function which configures/sets up each component requiring project specific input. This includes
 * allocating resources, passing-in arguments, etc. This function may be inlined into the topology setup function if
 * desired, but is extracted here for clarity.
 */
void configureTopology() {
    // Rate group driver needs a divisor list
    rateGroupDriver.configure(rateGroupDivisors);

    // Rate groups require context arrays.
    rateGroup5sec.configure(rateGroup5secContext, FW_NUM_ARRAY_ELEMENTS(rateGroup5secContext));
    rateGroup6Hz.configure(rateGroup6HzContext, FW_NUM_ARRAY_ELEMENTS(rateGroup6HzContext));

    // Framer and Deframer components need to be passed a protocol handler
    framer.setup(framing);
    deframer.setup(deframing);

    // TODO is this proper? Doing sensor setup in the constructor setup appears to fail because it is to early in boot
    // Sensors need to be initialized
    gncManager.initialize();
}

// Public functions for use in main program are namespaced with deployment name Fauro
namespace Fauro {
void setupTopology(const TopologyState& state) {
    // Autocoded initialization. Function provided by autocoder.
    initComponents(state);
    // Autocoded id setup. Function provided by autocoder.
    setBaseIds();
    // Autocoded connection wiring. Function provided by autocoder.
    connectComponents();
    // Autocoded command registration. Function provided by autocoder.
    regCommands();
    // Project-specific component configuration. Function provided above. May be inlined, if desired.
    configureTopology();
    // Autocoded parameter loading. Function provided by autocoder.
    // loadParameters();
    // Autocoded task kick-off (active components). Function provided by autocoder.
    startTasks(state);
    
    rateDriver.configure(1);
    commDriver.configure(&Serial);
    rateDriver.start();
}

void teardownTopology(const TopologyState& state) {
    // Autocoded (active component) task clean-up. Functions provided by topology autocoder.
    stopTasks(state);
    freeThreads(state);
}
};  // namespace Fauro
