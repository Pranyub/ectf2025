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
}


