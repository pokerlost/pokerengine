from __future__ import annotations

from enum import Enum

class PositionE(Enum):
    """
    Represents player position in the game.
    """

    NONE = -1
    SB = 0
    BB = 1
    UTG = 2
    LWJ = 3
    HIJ = 4
    COF = 5
    BTN = 6
