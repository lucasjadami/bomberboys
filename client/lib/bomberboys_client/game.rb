module BomberboysClient
  class Game
    def initialize
      @players = {}
      @bombs   = {}
      @bombs_mutex = Mutex.new
      @players_mutex = Mutex.new
    end

    def add_player(id, x, y, name)
      @players_mutex.synchronize do
        player = Player.new(id, name)
        player.move(x, y)

        @local_id = id if @players.empty?
        @players[player.id] = player
      end
    end

    def remove_player(id)
      @players_mutex.synchronize do
        @players.delete(id)
      end
    end

    def move_player(id, x, y)
      @players_mutex.synchronize do
        @players[id].move(x, y)
      end
    end

    def fall_player(id)
      @players_mutex.synchronize do
        @board.players[id].kill
      end
    end

    def add_bomb(id, x, y)
      @bombs_mutex.synchronize do
        @bombs[id] = Bomb.new(id, x, y)
      end
    end

    def explode_bomb(id)
      @bombs_mutex.synchronize do
        @bombs[id].explode
      end
    end

    def bombs
      @bombs_mutex.synchronize do
        @bombs.values
      end
    end

    def dangerous_bombs
      self.bombs.select { |b| distance(b.position, local_position) < b.explosion_radius }
    end

    private
    def distance(obj1, obj2)
      pairs = obj1.zip(obj2)
      sum = pairs.reduce(0) { |sum, el| sum + (el[0] - el[1])**2 }

      Math.sqrt(sum)
    end

    def local_position
      @players_mutex.synchronize do
        @players[@local_id].position
      end
    end
  end
end
