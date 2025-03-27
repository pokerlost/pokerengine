from __future__ import annotations

from enum import Enum

class ActionE(Enum):
    """
    Represents player action.
    """

    NONE = -1
    FOLD = 0
    CHECK = 1
    CALL = 2
    BET = 3
    RAISE = 4
