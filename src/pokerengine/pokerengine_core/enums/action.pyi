from __future__ import annotations

from enum import Enum

class Action(Enum):
    """
    Represents player action.
    """

    FOLD = 0
    CHECK = 1
    CALL = 2
    BET = 3
    RAISE = 4
