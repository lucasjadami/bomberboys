class Player
  attr_reader :name, :id

  def initialize(id, name)
    @x = @y = 0
    @dead = false
    @name = name
    @id = id
  end

  def move(x, y)
    @x, @y = x, y
  end

  def position
    [@x, @y]
  end

  def kill
    @dead = true
  end

  def dead?
    @dead
  end
end
