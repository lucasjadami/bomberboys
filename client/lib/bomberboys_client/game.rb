module BomberboysClient
  class Game
    attr_reader :players
    attr_reader :bombs
    attr_reader :local_id
    def initialize
      @players = {}
      @bombs   = {}
      @bombs_mutex = Mutex.new
      @players_mutex = Mutex.new
    end

    def add_player(id, x, y, name)
<<<<<<< HEAD
      player = Player.new(id, name)
      player.move(x, y)

      @board.add_player(player)
=======
      if x.nil? || y.nil?
        puts "ERROR: incomplete parameters => add_player #{id}, #{x}, #{y}, #{name}."
        return
      end

      @players_mutex.synchronize do
        player = Player.new(id, name)
        player.move(x, y)

        @local_id = id if @players.empty?
        @players[player.id] = player
      end
>>>>>>> feature/bot
    end

    def remove_player(id)
      @players_mutex.synchronize do
        @players.delete(id)
      end
    end

    def move_player(id, x, y)
      if x.nil? || y.nil?
        puts "ERROR: incomplete parameters => move_player #{id}, #{x}, #{y}."
        return
      end

      @players_mutex.synchronize do
        @players[id].move(x, y)
      end
    end

    def nearest_players
      players = @players_mutex.synchronize { @players.values - [local] }

      players.sort { |a, b| distance(local.position, a.position) <=> distance(local.position, b.position) }
    end

    def attackable_players
      nearest_players.select { |b| distance(b.position, local.position) < Bomb::RADIUS }
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

    def local_bomb
      @bombs[@local_id]
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
      self.bombs.select do |b|
        distance(b.position, local.position) < Bomb::RADIUS && !b.exploded?
      end
    end

    def local
      @players[@local_id]
    end

    private
    def distance(obj1, obj2)
      pairs = obj1.zip(obj2)
      sum = pairs.reduce(0) { |sum, el| sum + (el[0] - el[1])**2 }

      Math.sqrt(sum)
    end
  end
end
