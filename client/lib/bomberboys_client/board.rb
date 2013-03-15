class Board
  attr_reader :players, :bombs

  def initialize
    @players = {}
    @bombs   = {}
  end
end
