#include <stdio.h>
#include <stdlib.h>
#define MAIN_MODULE
#include "solitaire.h"

static char usage[] = "solitaire [ seed | filename ]\n";
static char couldnt_open[] = "couldn't open %s\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static void initialize_deck(struct pile *deck);
static void shuffle_deck(struct pile *deck);
static void print_pile(struct pile *pile);
static void print_card(char card,char delim);
static int play_solitaire(struct pile *deck,struct pile *ace_piles,struct pile *piles);
static void initialize_ace_piles(pile *ace_piles);
static void deal_to_piles(struct pile *deck,struct pile *piles);
static void print_ace_piles(struct pile *ace_piles,bool bOnlyFaceUp);
static void print_piles(struct pile *piles,bool bOnlyFaceUp);
static int build_ace_pile(struct pile *ace_piles,struct pile *piles);
static int start_king_pile(struct pile *piles);

int main(int argc,char **argv)
{
  int n;
  int seed;
  bool bShuffle;
  FILE *fptr;
  int rank;
  int suit;
  char card;
  int line_len;
  int line_no;
  struct pile deck;
  struct pile ace_piles[NUM_SUITS];
  struct pile piles[NUM_PILES];
  int retval;
  char cards_used[NUM_CARDS_IN_DECK];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((argv[1][0] >= '1') && (argv[1][0] <= '9')) {
    sscanf(argv[1],"%d",&seed);
    srand(seed);
    bShuffle = true;
  }
  else {
    if ((fptr = fopen(argv[1],"r")) == NULL) {
      printf(couldnt_open,argv[1]);
      return 2;
    }

    line_no = 0;

    for (n = 0; n < NUM_CARDS_IN_DECK; n++)
      cards_used[n] = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no > NUM_CARDS_IN_DECK) {
        printf("too many lines in file %s\n",argv[1]);
        return 3;
      }

      for (rank = 0; rank < NUM_CARDS_IN_SUIT; rank++) {
        if (line[0] == rank_chars[rank])
          break;
      }

      if (rank == NUM_CARDS_IN_SUIT) {
        printf("bad rank in line %d: %s\n",line_no,line);
        return 4;
      }

      for (suit = 0; suit < NUM_SUITS; suit++) {
        if (line[1] == suit_chars[suit])
          break;
      }

      if (suit == NUM_SUITS) {
        printf("bad rank in line %d: %s\n",line_no,line);
        return 5;
      }

      card = suit * NUM_CARDS_IN_SUIT + rank;

      if (cards_used[card]) {
        printf("card %d already used\n",card);
        return 6;
      }

      deck.pile[line_no - 1] = card;
      cards_used[card] = 1;
    }

    deck.pile_depth = NUM_CARDS_IN_DECK;
    deck.face_up_depth = 0;

    fclose(fptr);
    bShuffle = false;
  }

  if (bShuffle) {
    initialize_deck(&deck);
    shuffle_deck(&deck);
  }

  initialize_ace_piles(ace_piles);
  deal_to_piles(&deck,piles);
  printf("print_piles(piles,false);\n");
  print_piles(piles,false);

  retval = play_solitaire(&deck,ace_piles,piles);

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}

static void initialize_deck(struct pile *deck)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++)
    deck->pile[n] = n;

  deck->pile_depth = NUM_CARDS_IN_DECK;
  deck->face_up_depth = 0;
}

static void shuffle_deck(struct pile *deck)
{
  int n;
  int work;
  char temp;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
    work = rand() % NUM_CARDS_IN_DECK;

    if (work != n) {
      temp = deck->pile[work];
      deck->pile[work] = deck->pile[n];
      deck->pile[n] = temp;
    }
  }
}

static void print_pile(struct pile *pile)
{
  int n;

  for (n = 0; n < pile->pile_depth; n++)
    print_card(pile->pile[n],0x0a);
}

static void print_card(char card,char delim)
{
  int suit;
  int rank;

  suit = suit_of(card);
  rank = rank_of(card);

  printf("%c%c%c",rank_chars[rank],suit_chars[suit],delim);
}

static int play_solitaire(struct pile *deck,struct pile *ace_piles,struct pile *piles)
{
  int retval;

  for ( ; ; ) {
    retval = build_ace_pile(ace_piles,piles);

    if (retval) {
      printf("build_ace_pile print_ace_piles(ace_piles,false);\n");
      print_ace_piles(ace_piles,false);
      printf("print_piles(piles,false);\n");
      print_piles(piles,false);

      continue;
    }

    retval = start_king_pile(piles);

    if (retval) {
      printf("start_king_pile print_piles(piles,false);\n");
      print_piles(piles,false);

      continue;
    }

    break;
  }
}

static void initialize_ace_piles(pile *ace_piles)
{
  int n;

  for (n = 0; n < NUM_SUITS; n++) {
    ace_piles[n].pile_depth = 0;
    ace_piles[n].face_up_depth = 0;
  }
}

static void deal_to_piles(struct pile *deck,struct pile *piles)
{
  int m;
  int n;
  int p;

  for (n = 0; n < NUM_PILES; n++) {
    piles[n].pile_depth = n + 1;
    piles[n].face_up_depth = 1;
  }

  p = 0;

  for (m = 0; m < NUM_PILES; m++) {
    for (n = m; n < NUM_PILES; n++) {
      piles[n].pile[piles[n].pile_depth - 1 - m] = deck->pile[p++];
    }
  }

  for (n = 0; n < NUM_CARDS_IN_DECK - p; n++)
    deck->pile[n] = deck->pile[n + p];

  deck->pile_depth = NUM_CARDS_IN_DECK - p;
}

static void print_ace_piles(struct pile *ace_piles,bool bOnlyFaceUp)
{
  int m;
  int n;

  for (m = 0; m < NUM_SUITS; m++) {
    if (bOnlyFaceUp) {
      for (n = 0; n < ace_piles[m].face_up_depth; n++) {
        if (!n)
          printf("%d ",m);

        if (n < ace_piles[m].face_up_depth - 1)
          print_card(ace_piles[m].pile[n],' ');
        else
          print_card(ace_piles[m].pile[n],0x0a);
      }
    }
    else {
      for (n = 0; n < ace_piles[m].pile_depth; n++) {
        if (!n)
          printf("%d ",m);

        if (n < ace_piles[m].pile_depth - 1)
          print_card(ace_piles[m].pile[n],' ');
        else
          print_card(ace_piles[m].pile[n],0x0a);
      }
    }
  }
}

static void print_piles(struct pile *piles,bool bOnlyFaceUp)
{
  int m;
  int n;

  for (m = 0; m < NUM_PILES; m++) {
    if (bOnlyFaceUp) {
      for (n = 0; n < piles[m].face_up_depth; n++) {
        if (!n)
          printf("%d ",m);

        if (n < piles[m].face_up_depth - 1)
          print_card(piles[m].pile[n],' ');
        else
          print_card(piles[m].pile[n],0x0a);
      }
    }
    else {
      for (n = 0; n < piles[m].pile_depth; n++) {
        if (!n)
          printf("%d ",m);

        if (n < piles[m].pile_depth - 1) {
          if (n == piles[m].face_up_depth - 1)
            print_card(piles[m].pile[n],'/');
          else
            print_card(piles[m].pile[n],' ');
        }
        else
          print_card(piles[m].pile[n],0x0a);
      }
    }
  }
}

static int build_ace_pile(struct pile *ace_piles,struct pile *piles)
{
  int m;
  int n;
  char card;
  int rank;
  int suit;
  int compare_rank;
  int compare_suit;

  for (suit = CLUBS; suit < NUM_SUITS; suit++) {
    rank = -1;

    if (!ace_piles[suit].pile_depth)
      rank = ACE;
    else {
      rank = rank_of(ace_piles[suit].pile[0]);

      if (rank < KING)
        rank++;
    }

    if (rank == -1)
      continue;

    for (n = 0; n < NUM_PILES; n++) {
      if (piles[n].pile_depth) {
        card = piles[n].pile[0];
        compare_rank = rank_of(card);
        compare_suit = suit_of(card);

        if ((compare_rank == rank) && (compare_suit == suit)) {
          for (m = ace_piles[suit].pile_depth; m > 0; m--)
            ace_piles[suit].pile[m] = ace_piles[suit].pile[m-1];

          ace_piles[suit].pile[0] = card;
          ace_piles[suit].pile_depth++;
          ace_piles[suit].face_up_depth++;

          for (m = 0; m < piles[n].pile_depth - 1; m++)
            piles[n].pile[m] = piles[n].pile[m+1];

          piles[n].pile_depth--;

          if (piles[n].face_up_depth > 1)
            piles[n].face_up_depth--;
          else if (!piles[n].pile_depth)
            piles[n].face_up_depth = 0;

          return 1;
        }
      }
    }
  }

  return 0;
}

static int start_king_pile(struct pile *piles)
{
  int m;
  int n;
  int p;
  int rank;

  for (n = 0; n < NUM_PILES; n++) {
    if (!piles[n].pile_depth) {
      for (m = 0; m < NUM_PILES; m++) {
        if ((piles[m].face_up_depth == 1) && (piles[m].pile_depth > 1)) {
          rank = rank_of(piles[m].pile[0]);

          if (rank == KING) {
            piles[n].pile[0] = piles[m].pile[0];
            piles[n].pile_depth = 1;
            piles[n].face_up_depth = 1;

            for (p = 0; p < piles[m].pile_depth - 1; p++)
              piles[m].pile[p] = piles[m].pile[p+1];

            piles[m].pile_depth--;

            if (!piles[m].pile_depth)
              piles[m].face_up_depth = 0;

            return 1;
          }
        }
      }
    }
  }

  return 0;
}
