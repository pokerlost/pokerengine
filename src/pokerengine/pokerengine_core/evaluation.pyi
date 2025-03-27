from __future__ import annotations

from typing import List, Tuple, overload

from pokerengine.pokerengine_core.card.card import Rank
from pokerengine.pokerengine_core.card.cards import Cards
from pokerengine.pokerengine_core.enums.combination import CombinationE

class Result:
    """
    Represents Poker hand straight.
    """

    @overload
    def __init__(
        self,
        type: CombinationE,
        major: int,
        minor: int,
        kickers: int,
    ) -> None: ...
    @overload
    def __init__(self, type: int, major: int, minor: int, kickers: int) -> None: ...
    def __eq__(self, other: Result) -> bool: ...
    def __gt__(self, other: Result) -> bool: ...
    def __lt__(self, other: Result) -> bool: ...
    def __ne__(self, other: Result) -> bool: ...
    def __str(self) -> str: ...
    def __int__(self) -> int:
        """
        Use this method to get result as bitset.

        :return: Result as bitset
        """
    @property
    def kickers(self) -> int:
        """
        Use this method to get kickers, which are used in combination.

        :return: Kickers (combination must be less than four cards)
        """
    @property
    def major_rank(self) -> Rank:
        """
        Use this method to get major rank.

        :return: Major rank
        """
    @property
    def minor_rank(self) -> Rank:
        """
        Use this method to get minor rank.

        :return: Minor rank
        """
    @property
    def result(self) -> int:
        """
        Use this method to get hand straight.

        :return: Hand straight
        """
    @property
    def type(self) -> int:
        """
        Use this method to get combination type.

        :return: Combination type
        """

def get_evaluation_result(cards: Cards, players: List[int]) -> List[Tuple[Result, int]]:
    """
    Use this function to get hand results.

    :param cards: Game cards, can be generated via :class:`pokerengine.card.CardGenerator`
    :param players: Eligible player indexes
    :return: List of winner result and player index
    """

def get_evaluation_result_one(cards: Cards, for_: int) -> Result:
    """
    Use this function to get hand result.

    :param cards: Game cards, can be generated via :class:`pokerengine.card.CardGenerator`
    :param for_: Player indexes in :method:`pokerengine.card.CardGenerator.get_hands`
    :return: Hand result for player
    """
