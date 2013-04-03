module BomberboysClient
  class Message
    ACTION = {
      :login => 0, :add_player => 1, :remove_player => 2, :move_me => 3, :move_player => 4,
      :plant_bomb => 5, :add_bomb => 6, :explode_bomb => 7, :fall_player => 8
    }
    MASK = {
      0 => 'A*', 1 => 'nnnA*', 2 => 'S', 3 => 'C', 4 => 'nnn', 5 => '', 6 => 'nnn', 7 => 'n', 8 => 'n'
    }

    attr_reader :params

    def initialize(action, params = [])
      @action = ACTION[action]
      @params = params
    end

    def action
      ACTION.key(@action)
    end

    def pack
      message = @params.unshift(@action)

      message.pack('C' << MASK[@action])
    end

    def to_s
      "#{self.action}: #{@params.join(" ")}"
    end

    def self.unpack(str)
      action, str_params = str.unpack('CA*')
      params = str_params.unpack(MASK[action])

      Message.new(ACTION.key(action), params)
    end
  end
end
