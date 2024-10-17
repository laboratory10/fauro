module Fauro {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {
    constant QUEUE_SIZE = 3
    constant STACK_SIZE = 64 * 1024
  }

  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------

  instance commandDispatcher: Svc.CommandDispatcher base id 0x0100 \
    queue size Default.QUEUE_SIZE\
    stack size Default.STACK_SIZE \
    priority 101

  instance eventLogger: Svc.ActiveLogger base id 0x0200 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 98

  instance tlmSend: Svc.TlmChan base id 0x0300 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 97

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  instance rateGroup5sec: Svc.PassiveRateGroup base id 0x1000

  instance rateGroup6Hz: Svc.PassiveRateGroup base id 0x1100

  instance telemetryManager: Components.TelemetryManager base id 0x4000

  instance framer: Svc.Framer base id 0x4100

  instance fatalAdapter: Svc.AssertFatalAdapter base id 0x4200

  instance fatalHandler: Components.FauroFatalHandler base id 0x4300

  instance rateGroupDriver: Svc.RateGroupDriver base id 0x4400

  instance staticMemory: Svc.StaticMemory base id 0x4500

  instance textLogger: Svc.PassiveTextLogger base id 0x4600

  instance deframer: Svc.Deframer base id 0x4700

  instance rateDriver: Arduino.HardwareRateDriver base id 0x4800

  instance fswManager: Components.FswManager base id 0x4900

  instance fauroTime: Components.FauroTime base id 0x4A00

  instance gncManager: Components.GncManager base id 0x4B00

  instance powerManager: Components.PowerManager base id 0x4C00

  instance prmDb: Components.PrmDb base id 0x4D00

}
