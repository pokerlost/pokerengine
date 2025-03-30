from __future__ import annotations

from enum import Enum

class State(Enum):
    """
    Represents player state in the game.
    """

    INIT = 0
    OUT = 1
    ALIVE = 2
    ALLIN = 3
