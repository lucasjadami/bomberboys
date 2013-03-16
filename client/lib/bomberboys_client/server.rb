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

    def send(action, parameters)
    end

    def register(game = Game.new)
    end

    def start
    end
  end
end
