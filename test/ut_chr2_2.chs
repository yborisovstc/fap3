root : Elem
{
  e1 : Elem;
  e1 < e1 @ Value = "Test value 1";
  # "Comment";
  e1 < {
      Value = "Test value 2";
      e1_1 : Elem;
      e1_2 : Elem;
      e1_2 < { e1_2_1 : Elem; }
  }
  e1 @ ep1_1 : e1_1;
  ep1_1 < e1 @ ep1_2 : e1_2;
}
