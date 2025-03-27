from __future__ import annotations

from pokerengine.card import Cards
from pokerengine.evaluation import get_evaluation_result_one


def main() -> None:
    print("Hello, I'll help you to get your hand straight")  # noqa
    cards = input("Enter your cards (split cards by space): ")  # noqa
    board = input("Enter the board (split cards by space): ")  # noqa

    front, back = cards.split()
    run(front=front, back=back, board=board)


def run(front: str, back: str, board: str) -> None:
    if len(board) <= 3:
        raise RuntimeError("Sorry, board size must be greater than 3")

    try:
        print(  # noqa
            str(
                get_evaluation_result_one(
                    cards=Cards(board=board.lower().split(), hands=[front.lower() + back.lower()]),
                    for_=0,
                )
            ).title()
        )
    except Exception:  # noqa
        print("Invalid input")  # noqa


if __name__ == "__main__":
    main()
