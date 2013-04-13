module BomberboysClient
  class DummyBot < LocalPlayer
    def initialize(server)
      super(server)
    end

    def react(world)
      local = world.local

      if local.x > 550
        move_me :w
      elsif local.x < 50
        move_me :e
      elsif local.y > 370
        move_me :s
      elsif local.y < 50
        move_me :n
      elsif bomb = world.dangerous_bombs.first
        move_me opposite_direction(local, bomb)
      elsif world.attackable_players.any?
        plant_bomb
        move_me(DIRECTIONS.sample)
      elsif (player = world.nearest_players.first) && world.local_bomb && world.local_bomb.exploded?
        move_me direction(local, player)
      end
    end

    private
    def opposite_direction(local, bomb)
      cardinal_direction(Math.atan2(bomb.y - local.y, bomb.x - local.x) + Math::PI)
    end

    def direction(local, obj)
      cardinal_direction(Math.atan2(obj.y - local.y, obj.x - local.x))
    end

    def cardinal_direction(angle)
      DIRECTIONS[(4*angle/Math::PI - 0.5).ceil - 1]
    end
  end
end
