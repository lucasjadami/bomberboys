module BomberboysClient
  class Server
    def initialize
      @server_actions = {
        "\x01" => :add_player,   "\x02" => :remove_player, "\x04" => :move_player,
        "\x06" => :add_bomb, "\x07" => :explode_bomb, "\x08" => :fall_player
      }
      @client_actions = {
        :login => "\x00", :move_me => "\x03", :plant_bomb => "\x05"
      }
    end

    def send_message(action, parameters)
      puts encoded(parameters.unshift(action))
    end

    def register(game)
      @game = game
    end

    def start
      message = ["\x06", "\x02", "\x01", "\x01"]
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
