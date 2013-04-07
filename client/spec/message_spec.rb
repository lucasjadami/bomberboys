require 'spec_helper'

describe Message do
  it 'pack data correctly' do
    message = Message.new(:login, ['asdf'])
    str = message.pack
    expect(str).to eq("\x00\x00\x00\x00\x00asdf                ")
  end

  it 'pack data correctly with trash' do
    message = Message.new(:login, ['asdf'], 5)
    str = message.pack
    expect(str).to eq("\x00\x00\x00\x00\x00asdf                *****")
  end

  it 'unpack data correctly' do
    message = Message.unpack("\x00\x00\x00\x00\x00asdf")
    expect(message).to eq(Message.new(:login, ["asdf"]))
  end

  it 'unpack data correctly with trash' do
    message = Message.unpack("\x00\x00\x00\x00\x00asdf                ***")
    expect(message).to eq(Message.new(:login, ["asdf"]))
  end

  it 'extract uid' do
    message = Message.unpack("\x00\x00\x00\x01\x00asdf                ***")
    expect(message.uid).to eq(1)
  end
end

