root : Elem
{
  # "This is chromo for experimantal syntax omitting mut separator"
  Cnt1 = "Test of content"
  e1 : Elem
  e1 <  e1_0 : Elem
  # "Comment"
  e1 < { e1_1 : Elem e1_2 : Elem e1_2 < { e1_2_1 : Elem } }
  e4 ~ e1 @ {
      # "Comment2"
  }
}
