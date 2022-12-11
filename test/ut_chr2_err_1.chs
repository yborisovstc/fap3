root : Elem
{
  e2 : e1 {
     e2_1 : Elem;
  }
  + my_mod;
  C1 : Content;
  Cnt1 = "Test of content";
  e1 : Elem;
  e1 <  e1_0 : Elem;
  # "Comment";
  e1 < { e1_1 : Elem; e1_2 : Elem; e1_2 < { e1_2_1 : Elem; } }
  e4 ~ e1 @ {
      # "Comment2";
  };
  ! e4;
  e4 !~ e1;
}
