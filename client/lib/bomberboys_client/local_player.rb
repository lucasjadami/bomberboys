module BomberboysClient
  class LocalPlayer 
    DIRECTIONS = [:ne, :n, :nw, :w, :sw, :s, :se, :e] 

    attr_reader :name
    attr_reader :socket

    def initialize(socket, name)
      @socket = socket
      @name = name
    end

    def react(world); end

    protected
    def move_me(direction)
      dir = { n:0, ne:1, e:2, se:3, s:4, sw:5, w:6, nw:7 }
      @socket.send(Message.new(:move_me, [dir[direction]]))
      sleep 0.2
    end

    def plant_bomb
      @socket.send(Message.new(:plant_bomb))
    end
  end
end
