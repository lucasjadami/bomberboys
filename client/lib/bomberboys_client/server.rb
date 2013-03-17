module BomberboysClient
  class Server
    def initialize
      @server_actions = {
        0x01 => :add_player,   0x02 => :remove_player, 0x04 => :move_player,
        0x06 => :add_bomb, 0x07 => :explode_bomb, 0x08 => :fall_player
      }
      @client_actions = {
        :login => 0x00, :move_me => 0x03, :plant_bomb => 0x05
      }
    end

    def send_message(action, parameters)
    end

    def register(game)
      @game = game
    end

    def start
      message = "h\n\n\n\n\n\nasdf"
      loop do
        game.send(@server_actions[message[0]], message[1..-1])
      end
    end
  end
end
