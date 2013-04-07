require 'spec_helper'

describe ServerInterface do
  it 'receive message from source' do
    message = Message.new(:login, ['asdf'])
    socket = double('Socket')
    socket.stub(:recv).and_return(message.pack)

    server = ServerInterface.new(socket)

    new_message = server.receive

    expect(new_message).to eq(message)
  end

  it 'receive delayed message' do
    m1 = Message.new(:login, ['asdf'], 0, 40)
    m2 = Message.new(:login, ['asdf'], 0, 30)
    m3 = Message.new(:login, ['asdf'], 0, 41)
    m4 = Message.new(:login, ['asdf'], 0, 31)

    socket = double('Socket')
    socket.stub(:recv).and_return(m1.pack, m2.pack, m3.pack, m4.pack)

    server = ServerInterface.new(socket)

    server.receive
    new_message = server.receive

    expect(new_message).to eq(m3)
  end

  it 'calculate lost packages' do
    m1 = Message.new(:login, ['asdf'], 0, 2)
    m2 = Message.new(:login, ['asdf'], 0, 1)
    m3 = Message.new(:login, ['asdf'], 0, 4)

    socket = double('Socket')
    socket.stub(:recv).and_return(m1.pack, m2.pack, m3.pack)

    server = ServerInterface.new(socket)

    server.receive
    new_message = server.receive

    expect(server.packet_loss).to eq(3)
  end
end
