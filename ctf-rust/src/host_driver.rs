use core::convert::Infallible;

use bincode::serde::DecodeError;

pub const MAX_MESSAGE_SIZE: usize = 0x100;
pub const BLOCK_SIZE: usize = 0x100;

#[derive(Eq, PartialEq, Clone)]
pub enum MessageType {
    Invalid,
    Decode,
    Subscribe,
    List,
    Ack,
    Error,
    Debug,
}

pub enum UartError {
  Decode(DecodeError),
  InvalidOpcode,
  InvalidLength,
}

pub enum UartState {
  None,
  NumBytesRead(usize),
}

pub struct MessageHeader {
  pub opcode: MessageType,
  pub length: u16,
}

impl MessageHeader {
  pub fn new() -> Self {
    Self {
      opcode: MessageType::Invalid,
      length: 0
    }
  }
  pub fn should_ack(&self) -> bool {
    match self.opcode {
      MessageType::Ack | MessageType::Debug => false,
      _ => true
    }
  }
}

pub struct Message {
  pub header: MessageHeader,
  pub data: [u8; MAX_MESSAGE_SIZE]
}

impl Message {
  pub fn new() -> Self {
    Self {
      header: MessageHeader {
        opcode: MessageType::Invalid,
        length: 0
      },
      data: [0u8; MAX_MESSAGE_SIZE]
    }
  }
  pub fn list() -> Self {
    Self {
      header: MessageHeader {
        opcode: MessageType::List,
        length: 0,
      },
      data: [0u8; MAX_MESSAGE_SIZE],
    }
  }
}

pub struct HostDriver<Serial, SerialError = Infallible>
  where Serial: embedded_hal_nb::serial::Read<u8, Error = SerialError> + embedded_hal_nb::serial::Write<u8, Error = SerialError>
{
  uart: Serial,
  state: UartState
}

impl<Serial, SerialError> HostDriver<Serial, SerialError>
where
    Serial: embedded_hal_nb::serial::Read<u8, Error = SerialError> + embedded_hal_nb::serial::Write<u8, Error = SerialError>,
{
  pub fn new(uart: Serial) -> Self {
    Self {
      uart,
      state: UartState::None
    }
  }
  // drop uart obj
  pub fn free(self) -> Serial {
    self.uart
  }
}
