module BomberboysClient
  class Game
    def initialize
      @board = Board.new
    end

    def add_player(params)
      id, x, y, name = params.unpack('SSSA*')
      player = Player.new(id, name, @board.players.empty?)
      player.move(x, y)
      @board.players[id] = player
    end

    def remove_player(params)
      id = params.unpack('S').first
      @board.delete(id)
    end

    def move_player(params)
      id, x, y = params.unpack('SSS')
      @board.players[id].move(x, y)
    end

    def fall_player(params)
      id = params.unpack('S').first
      @board.players[id].kill
    end

    def add_bomb(params)
      id, x, y = params.unpack('SSS')
      @board.bombs[id] = Bomb.new(id, x, y)
    end

    def explode_bomb(params)
      id = params.unpack('S').first
      @board.bombs[id].explode
    end
  end
end
