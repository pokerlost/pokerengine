from __future__ import annotations

from typing import List, Sequence, overload

import pybind11_stubgen.typing_ext
import typing_extensions
from pokerengine.pokerengine_core.enums.rank import RankE
from pokerengine.pokerengine_core.enums.suit import SuitE

class Rank:
    """
    Represents card rank.
    """

    @overload
    def __init__(self, value: RankE) -> None: ...
    @overload
    def __init__(self, value: str) -> None: ...
    def __eq__(self, other: Rank) -> bool: ...
    def __gt__(self, other: Rank) -> bool: ...
    def __lt__(self, other: Rank) -> bool: ...
    def __ne__(self, other: Rank) -> bool: ...
    def __str(self) -> str: ...
    def __int__(self) -> int:
        """
        Use this method to get rank as bitset.

        :return: Rank as bitset
        """
    @property
    def rank(self) -> int:
        """
        Use this method to get card rank.

        :return: Rank as integer
        """
    @property
    def enum(self) -> RankE:
        """
        Use this method to get card rank.

        :return: Rank as enum
        """

class Suit:
    """
    Represents card suit.
    """

    @overload
    def __init__(self, value: SuitE) -> None: ...
    @overload
    def __init__(self, value: str) -> None: ...
    def __eq__(self, other: Suit) -> bool: ...
    def __gt__(self, other: Suit) -> bool: ...
    def __lt__(self, other: Suit) -> bool: ...
    def __ne__(self, other: Suit) -> bool: ...
    def __str(self) -> str: ...
    def __int__(self) -> int:
        """
        Use this method to get suit as bitset.

        :return: Suit as bitset
        """
    @property
    def suit(self) -> int:
        """
        Use this method to get card suit.

        :return: Suit as integer
        """
    @property
    def enum(self) -> SuitE:
        """
        Use this method to get card suit.

        :return: Suit as enum
        """

class Card:
    """
    Represents card.
    """

    @overload
    def __init__(self, rank: Rank, suit: Suit) -> None: ...
    @overload
    def __init__(self, rank: RankE, suit: SuitE) -> None: ...
    @overload
    def __init__(self, card: int) -> None: ...
    @overload
    def __init__(self, value: str) -> None: ...
    def __eq__(self, other: Card) -> bool: ...
    def __gt__(self, other: Card) -> bool: ...
    def __lt__(self, other: Card) -> bool: ...
    def __ne__(self, other: Card) -> bool: ...
    def __str(self) -> str: ...
    def __int__(self) -> int:
        """
        Use this method to get card as bitset.

        :return: Card as bitset
        """
    @property
    def card(self) -> int:
        """
        Use this method to get card.

        :return: Card as integer
        """
    @property
    def rank(self) -> Rank:
        """
        Use this method to get card rank.

        :return: Card rank
        """
    @property
    def suit(self) -> Suit:
        """
        Use this method to get card suit.

        :return: Card suit
        """

class CardGenerator:
    """
    Allow user generate cards.
    """

    def __init__(self, seed: int = 1927) -> None: ...
    def generate(self) -> Card:
        """
        Use this method to generate single card.

        :return: Generated card
        """
    def generate_v(self, n: int) -> List[Card]:
        """
        Use this method to generated cards. To generate game cards (where players size equals seven), use: generate_v(5 + 7 * 2).

        :return: Generated cards.
        """

class CardSet:
    """
    Represents card set.
    """

    cards: int

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, cards: Sequence[Card]) -> None: ...
    @overload
    def __init__(self, bitset: int) -> None: ...
    @overload
    def __init__(self, value: str) -> None: ...
    @overload
    def __add__(self, other: Card) -> CardSet:
        """
        Use this method to add card to card set.

        :param other: Value to add
        :return: New card set
        """
    @overload
    def __add__(self, other: CardSet) -> CardSet:
        """
        Use this method to add card set to card set.

        :param other: Value to add
        :return: New card set
        """
    @overload
    def __contains__(self, value: Card) -> bool:
        """
        Use this method to get is value in the card set.

        :param value: Value to check
        :return: Value in the card set
        """
    @overload
    def __contains__(self, value: CardSet) -> bool:
        """
        Use this method to get is card set in the card set.

        :param value: Card set to check
        :return: Card set in the card set
        """
    def __len__(self) -> int: ...
    @overload
    def remove(self, value: Card) -> None:
        """
        Use this method to remove card from card set.

        :param value: Card to remove
        :return: :class:`None`
        """
    @overload
    def remove(self, value: CardSet) -> None:
        """
        Use this method to remove card set from card set.

        :param value: Card set to remove
        :return: :class:`None`
        """
    def fill(self) -> None:
        """
        Use this method to fill card set with all cards.

        :return: :class:`None`
        """
    def clear(self) -> None:
        """
        Use this method to clear card set.

        :return: :class:`None`
        """
    def insert(self, value: Card) -> None:
        """
        Use this method to insert card into card set

        :return: :class:`None`
        """
    def join(self, value: CardSet) -> None:
        """
        Use this method to join two card sets.

        :return: :class:`None`
        """

class Cards:
    """
    Represents game cards.
    """

    @overload
    def __init__(self, board: List[str], hands: List[str]) -> None: ...
    @overload
    def __init__(self, cards: List[Card]) -> None: ...
    def __eq__(self, other: Cards) -> bool: ...
    def __gt__(self, other: Cards) -> bool: ...
    def __lt__(self, other: Cards) -> bool: ...
    def __ne__(self, other: Cards) -> bool: ...
    def __str(self) -> str: ...
    @property
    def board(self) -> List[Card]:
        """
        Use this method to get game board.

        :return: Game board
        """
    @property
    def hands(self) -> List[Hand]:
        """
        Use this method to get players hands.

        :return: Players hands
        """
    def board_n(self, n: int) -> List[Card]:
        """
        Use this method to get n game board.

        :return: N game board
        """

class Hand:
    """
    Represents texas holdem poker hand.
    """

    @overload
    def __init__(
        self,
        front: Card,
        back: Card,
    ) -> None: ...
    @overload
    def __init__(self, front: int, back: int) -> None: ...
    @overload
    def __init__(self, value: str) -> None: ...
    def __eq__(self, other: Hand) -> bool: ...
    def __gt__(self, other: Hand) -> bool: ...
    def __lt__(self, other: Hand) -> bool: ...
    def __ne__(self, other: Hand) -> bool: ...
    def __str(self) -> str: ...
    def __int__(self) -> int:
        """
        Use this method to get hand as bitset.

        :return: Hand as bitset.
        """
    @property
    def value(
        self,
    ) -> typing_extensions.Annotated[List[Card], pybind11_stubgen.typing_ext.FixedSize(2)]:
        """
        Use this method to get hand.

        :return: Texas holdem poker hand
        """
