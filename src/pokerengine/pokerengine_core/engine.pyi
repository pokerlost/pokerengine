from __future__ import annotations

from typing import List, Optional, Tuple

from pokerengine.pokerengine_core.card.cards import Cards
from pokerengine.pokerengine_core.enums.action import Action
from pokerengine.pokerengine_core.enums.position import Position
from pokerengine.pokerengine_core.enums.round import Round
from pokerengine.pokerengine_core.enums.state import State
from pokerengine.pokerengine_core.evaluation.result import Result

class Player:
    """
    Represents player in the game.
    """

    stack: int
    """Player stacksize."""
    bet: int
    """Player bet for the game."""
    round_bet: int
    """Player bet for the round."""
    state: State
    """Player state in round."""
    id: str
    """Player ID."""

    def __init__(
        self,
        stack: int,
        bet: int,
        round_bet: int,
        state: State,
        id: str,
    ) -> None: ...
    def __eq__(self, other: Player) -> bool: ...
    def __str__(self) -> str: ...

class PlayerAction:
    """
    Represents player action for execute.
    """

    amount: int
    action: Action
    position: Position

    def __init__(
        self,
        amount: int,
        action: Action,
        position: Position,
    ) -> None: ...
    def __eq__(self, other: PlayerAction) -> bool: ...
    def __str__(self) -> str: ...

class EngineTraits:
    """
    Game create settings.
    """

    sb_bet: int
    """Small blind bet."""
    bb_bet: int
    """Big blind bet."""
    bb_mult: int
    """Big blind multiplication, used in the formula: stack_to_join=bb_bet*bb_mult"""
    min_raise: int
    """Game minimal raise size."""

    def __init__(self, sb_bet: int, bb_bet: int, bb_mult: int, min_raise: int = 0) -> None: ...

class Engine:
    engine_traits: EngineTraits

    def __init__(
        self,
        engine_traits: EngineTraits,
        current: Optional[Position] = None,
        round: Optional[Round] = None,
        flop_dealt: Optional[bool] = None,
        players: Optional[List[Player]] = None,
    ) -> None: ...
    @property
    def current(self) -> Position:
        """
        Use this method to get current player position.

        :return: Current player position
        """
    @property
    def current_player(self) -> Player:
        """
        Use this method to get current player.

        :return: Current player
        """
    @property
    def flop_dealt(self) -> bool:
        """
        Use this method to get is flop dealt.

        :return: Flop dealt
        """
    @property
    def highest_bet(self) -> int:
        """
        Use this method to get the highest bet in the current round.

        :return: Highest round bet
        """
    @property
    def highest_game_bet(self) -> int:
        """
        Use this method to get the highest bet in the whole game.

        :return: Highest bet
        """
    @property
    def players(self) -> List[Player]:
        """
        Use this method to get players.

        :return: Players
        """
    @property
    def possible_actions(self) -> List[PlayerAction]:
        """
        Use this method to get possible actions.

        :return: Possible actions
        """
    @property
    def round(self) -> Round:
        """
        Use this method to get current game round.

        :return: Round
        """
    @property
    def pot(self) -> int:
        """
        Use this method to get game pot (rake adjusted).

        :return: Pot (rake adjusted)
        """
    @property
    def showdown(self) -> bool:
        """
        Use this method to get is game is showdown.

        :return: Is game is showdown
        """
    @property
    def terminal_state(self) -> bool:
        """
        Use this method to get game state.

        :return: Is number alive is greater than one
        """
    def start(self, is_new_game: bool = False) -> None:
        """
        Use this method to start/restart game.

        :param is_new_game: Is need to reset round
        :return: :class:`None`
        """
    def stop(self) -> None:
        """
        Use this method to stop game.

        :return: :class:`None`
        """
    def add_player(self, stack: int, id: str) -> None:
        """
        Use this method to add player in the game.

        :param stack: Player stack in the game
        :param id: Player ID
        :return: :class:`None`
        """
    def pay(self, cards: Cards) -> List[Tuple[Result, int]]:
        """
        Use this method to pay pot for players (with cards).

        :param cards: Game cards, can be generated via :class:`pokerengine.card.CardGenerator`
        :return: List of hand straight and pot
        """
    def pay_noshowdown(self) -> List[int]:
        """
        Use this method to pay pot for players (only one player left, other players are outed).

        :return: List of pot
        """
    def execute(self, player_action: PlayerAction) -> None:
        """
        Use this method to execute player action.

        :param player_action: Action to execute
        :return: :class:`None`
        """

class EngineRake01(Engine): ...
