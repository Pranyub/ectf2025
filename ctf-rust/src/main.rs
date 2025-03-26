#![no_std]
#![no_main]
use cortex_m::peripheral::{syst, Peripherals};
use cortex_m_rt::entry;
use panic_halt as _;
use cortex_m_semihosting::{debug, hprintln};

fn fib(n: u64) -> u64 {
    if n > 0 {
        n * fib(n - 1)
    } else {
        1
    }
}

#[entry]
fn main() -> ! {
    let peripherals = Peripherals::take().unwrap();

    hprintln!("hello world: {}", fib(5));

    while (1) {
        hprintln!("Ready");
    }

    debug::exit(debug::EXIT_SUCCESS);

    loop {}
}