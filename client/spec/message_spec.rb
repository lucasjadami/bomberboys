require 'spec_helper'

describe Message do
  it 'pack data correctly' do
    message = Message.new(:login, ['asdf'])
    str = message.pack
    expect(str).to eq("\x00asdf")
  end

  it 'unpack data correctly' do
    message = Message.unpack("\x00asdf")
    expect(message).to eq(Message.new(:login, ["asdf"]))
  end
end

