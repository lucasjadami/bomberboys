module BomberboysClient
  class Message
    ACTION = {
      :login         => 0, :add_player    => 1,  :remove_player => 2,
      :move_me       => 3, :move_player   => 4,  :plant_bomb    => 5,
      :add_bomb      => 6, :explode_bomb  => 7,  :fall_player   => 8,
      :ack           => 9, :ping          => 10, :pong          => 11
    }

    MASK = {
      0 => 'A*',  1 => 'nnnA*', 2 => 'S',
      3 => 'C',   4 => 'nnn',   5 => '',
      6 => 'nnn', 7 => 'n',     8 => 'n',
      9 => '',    10 => '',     11 => ''
    }

    attr_reader :params
    attr_accessor :uid

    def initialize(action, params = [], trash_size = 0, uid = 0)
      @action     = ACTION[action]
      @uid        = uid
      @trash_size = trash_size

      @params     = params
      @params[0]  = @params[0].ljust(20) if action == :login
    end

    def action
      ACTION.key(@action)
    end

    def pack
      message = [@uid, @action] + @params

      str_message = message.pack('NC' << MASK[@action])

      str_message << '*' * @trash_size
    end

    def to_s
      "#{self.action}: #{@params.join(" ")}"
    end

    def self.unpack(str)
      uid, action, str_params = str.unpack('NCA*')

      raise "Unknown action" unless ACTION.has_value?(action)

      params = str_params.unpack(MASK[action])

      raise "Malformed params for #{ACTION.key(action)}: #{params}" if params.include?(nil)

      params[0] = params[0][0..19] if ACTION.key(action) == :login
      Message.new(ACTION.key(action), params, 0, uid)
    end

    def ==(obj)
      self.params == obj.params && self.action == obj.action && @uid == obj.uid
    end
  end
end
