class MessageHandler
  def initialize
    @actions = {
      "\x00" => :login,    "\x01" => :add_player,   "\x02" => :remove_player,
      "\x03" => :move_me,  "\x04" => :move_player,  "\x05" => :plant_bomb,
      "\x06" => :add_bomb, "\x07" => :explode_bomb, "\x10" => :fall_player
    }
  end

  def start
  end

  def on_message(action, body)
  end
end
