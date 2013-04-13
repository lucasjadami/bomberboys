module BomberboysClient
  class RandomBot < LocalPlayer
    def initialize(server)
      super(server)
    end

    def react(world)
      plant_frequency.times { move_me DIRECTIONS.sample }
      plant_bomb
    end

    private
    def plant_frequency
      10 + rand(20)
    end
  end
end
