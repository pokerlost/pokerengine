from __future__ import annotations

import abc
from typing import Any, Generic, List, Self, TypeVar

from pokerengine import enums
from pokerengine.card import Card as CardOriginal
from pokerengine.card import Cards as CardsOriginal
from pokerengine.card import Hand as HandOriginal
from pokerengine.card import Rank as RankOriginal
from pokerengine.card import Suit as SuitOriginal
from pokerengine.engine import EngineRake01 as EngineRake01Original
from pokerengine.engine import EngineTraits as EngineTraitsOriginal
from pokerengine.engine import Player as PlayerOriginal
from pokerengine.engine import PlayerAction as PlayerActionOriginal
from pokerengine.pokerengine_core.enums.action import Action
from pokerengine.pokerengine_core.enums.position import Position
from pokerengine.pokerengine_core.enums.rank import Rank as RankEnum
from pokerengine.pokerengine_core.enums.round import Round
from pokerengine.pokerengine_core.enums.state import State
from pokerengine.pokerengine_core.enums.suit import Suit as SuitEnum
from pydantic import BaseModel, ConfigDict

PokerEngineType = TypeVar("PokerEngineType", bound=Any)


class _BaseModel(abc.ABC, BaseModel, Generic[PokerEngineType]):
    @classmethod
    def from_original(cls, value: PokerEngineType) -> Self:
        return cls.model_validate(value)

    @abc.abstractmethod
    def to_original(self) -> PokerEngineType:
        ...

    model_config = ConfigDict(
        arbitrary_types_allowed=True,
        populate_by_name=True,
        from_attributes=True,
    )


class Rank(_BaseModel[RankOriginal]):
    enum: RankEnum
    rank: int
    string: str

    @classmethod
    def from_original(cls, value: RankOriginal) -> Self:
        return Rank(
            enum=value.enum.value,
            rank=value.rank,
            string=value.string,
        )

    def to_original(self) -> RankOriginal:
        return RankOriginal(self.string)


class Suit(_BaseModel[SuitOriginal]):
    enum: SuitEnum
    suit: int
    string: str

    @classmethod
    def from_original(cls, value: SuitOriginal) -> Self:
        return Suit(
            enum=value.enum.value,
            suit=value.suit,
            string=value.string,
        )

    def to_original(self) -> SuitOriginal:
        return SuitOriginal(self.string)


class Card(_BaseModel[CardOriginal]):
    card: int
    string: str

    def to_original(self) -> CardOriginal:
        return CardOriginal(self.string)


class Hand(_BaseModel[HandOriginal]):
    front: Card
    back: Card

    @classmethod
    def from_original(cls, value: HandOriginal) -> Self:
        return Hand(
            front=Card(card=value.value[0].card, string=value.value[0].string),
            back=Card(card=value.value[1].card, string=value.value[1].string),
        )

    def to_original(self) -> PokerEngineType:
        return HandOriginal(self.front.string + self.back.string)


class Cards(_BaseModel[CardsOriginal]):
    board: List[Card]
    hands: List[Hand]

    @classmethod
    def from_original(cls, value: CardsOriginal) -> Self:
        return Cards(
            board=[Card(card=card.card, string=card.string) for card in value.board],
            hands=[
                Hand(
                    front=Card(card=hand.value[0].card, string=hand.value[0].string),
                    back=Card(card=hand.value[1].card, string=hand.value[1].string),
                )
                for hand in value.hands
            ],
        )

    def to_original(self) -> CardsOriginal:
        return CardsOriginal(
            board=[card.string for card in self.board],
            hands=[hand.front.string + hand.back.string for hand in self.hands],
        )


class EngineTraits(_BaseModel[EngineTraitsOriginal]):
    sb_bet: int
    bb_bet: int
    bb_mult: int
    min_raise: int

    def to_original(self) -> EngineTraitsOriginal:
        return EngineTraitsOriginal(
            sb_bet=self.sb_bet,
            bb_bet=self.bb_bet,
            bb_mult=self.bb_mult,
            min_raise=self.min_raise,
        )


class Player(_BaseModel[PlayerOriginal]):
    id: str
    stack: int
    bet: int
    round_bet: int
    state: enums.State

    @classmethod
    def from_original(cls, value: PlayerOriginal) -> Self:
        return Player(
            id=value.id,
            stack=value.stack,
            bet=value.bet,
            round_bet=value.round_bet,
            state=value.state.value,
        )

    def to_original(self) -> PlayerOriginal:
        return PlayerOriginal(
            id=self.id,
            stack=self.stack,
            bet=self.bet,
            round_bet=self.round_bet,
            state=State(self.state.value),
        )


class PlayerAction(_BaseModel[PlayerActionOriginal]):
    action: enums.Action
    position: enums.Position
    amount: int

    @classmethod
    def from_original(cls, value: PlayerActionOriginal) -> Self:
        return PlayerActionOriginal(
            action=value.action.value,
            position=value.position.value,
            amount=value.amount,
        )

    def to_original(self) -> PlayerActionOriginal:
        return PlayerActionOriginal(
            action=Action(self.action.value),
            position=Position(self.position.value),
            amount=self.amount,
        )


class EngineRake01(_BaseModel[EngineRake01Original]):
    engine_traits: EngineTraits
    current: enums.Position
    round: enums.Round
    flop_dealt: bool
    players: List[Player]

    @classmethod
    def from_original(cls, value: EngineRake01Original) -> Self:
        return EngineRake01(
            engine_traits=EngineTraits.from_original(value.engine_traits),
            current=value.current.value,
            round=value.round.value,
            flop_dealt=value.flop_dealt,
            players=[Player.from_original(player) for player in value.players],
        )

    def to_original(self) -> EngineRake01Original:
        return EngineRake01Original(
            engine_traits=EngineTraitsOriginal(
                sb_bet=self.engine_traits.sb_bet,
                bb_bet=self.engine_traits.bb_bet,
                bb_mult=self.engine_traits.bb_mult,
                min_raise=self.engine_traits.min_raise,
            ),
            current=Position(self.current.value),
            round=Round(self.round.value),
            flop_dealt=self.flop_dealt,
            players=[
                PlayerOriginal(
                    id=player.id,
                    stack=player.stack,
                    bet=player.bet,
                    round_bet=player.round_bet,
                    state=State(player.state.value),
                )
                for player in self.players
            ],
        )
