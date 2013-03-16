module BomberboysClient
  class Game
    def initialize
      @board = Board.new
    end

    def add_player(id, name)
      @board.players[player.id] = Player.new(id, name, @board.players.empty?)
    end

    def remove_player(id)
      @board.delete(id)
    end

    def move_player(id, x, y)
      @board.players[id].move(x, y)
    end

    def fall_player(id)
      @board.players[id].kill
    end

    def add_bomb(id, x, y)
      @board.bombs[id] = Bomb.new(id, x, y)
    end

    def explode_bomb(id)
      @board.bombs[id].explode
    end
  end
end
