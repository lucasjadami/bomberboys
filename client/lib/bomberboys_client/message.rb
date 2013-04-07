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
      message = [@action] + @params
      message.pack('C' << MASK[@action])
    end

    def to_s
      "#{self.action}: #{@params.join(" ")}"
    end

    def self.unpack(str)
      action, str_params = str.unpack('CA*')

      raise "Unknown action #{action}" unless ACTION.kas_value?(action)

      action = ACTION.key(action)
      params = str_params.unpack(MASK[action])

      raise "Malformed params for #{action}: #{params}" if params.include?(nil)

      Message.new(action, params)
    end

    def ==(obj)
      self.params == obj.params && self.action == obj.action
    end
  end
end
