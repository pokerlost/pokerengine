from __future__ import annotations

from enum import Enum

class Combination(Enum):
    """
    Represents poker combination.
    """

    NO_PAIR = 0
    ONE_PAIR = 1
    TWO_PAIR = 2
    THREE_OF_A_KIND = 3
    STRAIGHT = 4
    FLUSH = 5
    FULL_HOUSE = 6
    FOUR_OF_A_KIND = 7
    STRAIGHT_FLUSH = 8
