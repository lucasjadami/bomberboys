require 'spec_helper'

describe Message do
  it 'pack data correctly' do
    message = Message.new(:login, ['asdfg123451234512345'])
    str = message.pack
    expect(str).to eq("\x00\x00\x00\x00\x00asdfg123451234512345")
  end

  it 'unpack data correctly' do
    message = Message.unpack("\x00\x00\x00\x00\x00asdfg123451234512345")
    expect(message).to eq(Message.new(:login, ["asdfg123451234512345"]))
  end

  it 'extract uid' do
    message = Message.unpack("\x00\x00\x00\x01\x00asdfg123451234512345")
    expect(message.uid).to eq(1)
  end
end

