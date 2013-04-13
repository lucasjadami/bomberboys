module BomberboysClient
  class Message
    ACTION = {
      :login         =>  0, :add_player    =>  1,  :remove_player => 2,
      :move_me       =>  3, :move_player   =>  4,  :plant_bomb    => 5,
      :add_bomb      =>  6, :explode_bomb  =>  7,  :fall_player   => 8,
      :ack           =>  9, :ping          => 10,  :pong          => 11
      :info          => 12, :shutdown      => 13
    }

    MASK = {
      0 => 'A*',  1 => 'nnnA*', 2 => 'n',
      3 => 'C',   4 => 'nnn',   5 => '',
      6 => 'nnn', 7 => 'n',     8 => 'n',
      9 => '',    10 => '',     11 => ''
      12 => 'qqqq',   13 => ''
    }

    BODY_SIZE = {
      0 => 20,  1 => 26,  2 => 2,
      3 => 1,   4 => 6,   5 => 0,
      6 => 6,   7 => 2,   8 => 2,
      9 => 0,  10 => 0,  11 => 0
    }

    attr_reader :params
    attr_accessor :uid

    def initialize(action, params = [], uid = 0)
      @uid    = uid
      @action = action.kind_of?(Symbol) ? ACTION[action] : action
      @params = params.kind_of?(Array) ? params : unpack_body(@action, params)
    end

    def action
      ACTION.key(@action)
    end

    def pack
      message = [@uid, @action] + @params

      str_message = message.pack('NC' << MASK[@action])
    end

    def to_s
      "#{@uid}:#{self.action}:#{@params.join(" ")}"
    end

    def self.unpack(str)
      uid, action, str_params = str.unpack('NCA*')

      Message.new(ACTION.key(action), str_params, uid)
    end

    def ==(obj)
      self.params == obj.params && self.action == obj.action && @uid == obj.uid
    end

    private
    def unpack_body(action, params)
      params.unpack(MASK[action])
    end
  end
end
