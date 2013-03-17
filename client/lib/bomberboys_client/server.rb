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
      puts encoded(parameters.unshift(action))
    end

    def register(game)
      @game = game
    end

    def start
      message = [0x06, 0x02, 0x01, 0x01]
      loop do
        game.send(@server_actions[message.first], *decoded(message.shift))
        puts message
      end
    end

    private
    def decoded(array)
      array.map { |p| p.join('') }
    end

    def encoded(array)
    end
  end
end
