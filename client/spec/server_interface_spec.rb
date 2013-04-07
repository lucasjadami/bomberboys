require 'spec_helper'

describe ServerInterface do
  it 'read message from source' do
    message = Message.new(:login, ['asdf'])
    socket = double('Socket')
    socket.stub(:recv).and_return(message.pack)

    server = ServerInterface.new(socket)

    new_message = server.receive

    expect(new_message).to eq(message)
  end
end
