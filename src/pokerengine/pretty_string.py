from __future__ import annotations

import abc
from typing import Any

from pokerengine.enums import Rank, Suit
from pokerengine.schema import Card

__all__ = (
    "PrettyString",
    "PrettyCard",
)


class PrettyString(abc.ABC):
    @abc.abstractmethod
    def as_pretty_string(self, value: Any) -> str:
        ...


class PrettyCard:
    def __init__(
        self, clubs: str = "♣️", diamonds: str = "♦️", hearts: str = "♥️", spades: str = "♠️"
    ) -> None:
        self.clubs = clubs
        self.diamonds = diamonds
        self.hearts = hearts
        self.spades = spades

    def as_pretty_string(self, value: Card) -> str:
        card = value.to_original()

        pretty_string = Rank(card.rank.rank).name.capitalize()
        match Suit(card.suit.suit):
            case Suit.CLUBS:
                return pretty_string + self.clubs
            case Suit.DIAMONDS:
                return pretty_string + self.diamonds
            case Suit.HEARTS:
                return pretty_string + self.hearts
            case Suit.SPADES:
                return pretty_string + self.spades
