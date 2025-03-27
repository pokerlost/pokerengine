from __future__ import annotations

from typing import Final

from . import card, engine, enums, evaluation, utils

__version__: Final[str] = ...
__doc__: Final[
    str
] = """
    Poker library
    """

__all__ = (
    "__version__",
    "__doc__",
    "card",
    "engine",
    "enums",
    "evaluation",
    "utils",
)
