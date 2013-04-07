require 'spec_helper'

describe ServerInterface do
  it 'receive message from source' do
    message = Message.new(:login, ['asdfg123451234512345'])
    socket = double('Socket')
    socket.stub(:recv).and_return("\x00" * 5, 'asdfg123451234512345', '')

    server = ServerInterface.new(socket, 0)

    new_message = server.receive

    expect(new_message).to eq(message)
  end

  it 'receive delayed message' do
    m1 = Message.new(:login, ['asdfg123451234512345'], 5)
    m2 = Message.new(:login, ['asdfg123451234512345'], 3)
    m3 = Message.new(:login, ['asdfg123451234512345'], 6)
    m4 = Message.new(:login, ['asdfg123451234512345'], 4)

    socket = double('Socket')
    socket.stub(:recv).and_return("\x00\x00\x00\x05\x00", 'asdfg123451234512345', '',
                                  "\x00\x00\x00\x03\x00", 'asdfg123451234512345', '',
                                  "\x00\x00\x00\x06\x00", 'asdfg123451234512345', '',
                                  "\x00\x00\x00\x04\x00", 'asdfg123451234512345', '')

    server = ServerInterface.new(socket)

    server.receive
    new_message = server.receive

    expect(new_message).to eq(m3)
  end

  it 'calculate lost packages' do
    m1 = Message.new(:login, ['asdfg123451234512345'], 2)
    m2 = Message.new(:login, ['asdfg123451234512345'], 1)
    m3 = Message.new(:login, ['asdfg123451234512345'], 4)

    socket = double('Socket')
    socket.stub(:recv).and_return("\x00\x00\x00\x02\x00", 'asdfg123451234512345', '',
                                  "\x00\x00\x00\x01\x00", 'asdfg123451234512345', '',
                                  "\x00\x00\x00\x04\x00", 'asdfg123451234512345', '')

    server = ServerInterface.new(socket)

    server.receive
    new_message = server.receive

    expect(server.packet_loss).to eq(3)
  end
end
