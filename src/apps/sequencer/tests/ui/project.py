import testframework as tf

class ProjectPageTest(tf.UiTest):

    def test_edit_name(self):
        c = self.controller
        p = self.env.sequencer.model.project

        self.assertEqual(p.name, "INIT", "initial name")

        # edit -> cancel
        c.encoder().press("f4").wait()
        self.assertEqual(p.name, "INIT", "edit -> cancel")

        # edit -> clear -> ok
        c.encoder().press("f3").press("f5").wait()
        self.assertEqual(p.name, "", "edit -> clear -> ok")

        # edit -> a -> b -> c -> ok
        c.encoder().encoder().right().encoder().right().encoder().press("f5").wait()
        self.assertEqual(p.name, "ABC", "edit -> write -> ok")

        # edit -> backspace -> ok
        c.encoder().press("f1").wait().press("f5").wait()
        self.assertEqual(p.name, "AB", "edit -> backspace -> ok")

        # edit -> prev -> prev -> delete -> ok
        c.encoder().press("prev").press("prev").press("f2").press("f5").wait()
        self.assertEqual(p.name, "B", "edit -> prev -> prev -> delete -> ok")

    def test_edit_tempo(self):
        c = self.controller
        p = self.env.sequencer.model.project

        # initial tempo
        self.assertEqual(p.tempo, 120, "initial tempo")

        # select tempo
        c.right()

        # increase
        c.encoder().right().encoder().wait()
        self.assertEqual(p.tempo, 121, "increase")

        # decrease
        c.encoder().left().encoder().wait()
        self.assertEqual(p.tempo, 120, "decrease")

        # shift + increase
        c.encoder().down("shift").right().up("shift").encoder().wait()
        self.assertAlmostEqual(p.tempo, 120.1, places=1, msg="shift + increase")

        # shift + decrease
        c.encoder().down("shift").left().up("shift").encoder().wait()
        self.assertAlmostEqual(p.tempo, 120, places=1, msg="shift + decrease")

        # minimum
        p.tempo = 1
        c.encoder().left().encoder()
        self.assertEqual(p.tempo, 1, "minimum")

        # maximum
        p.tempo = 1000
        c.encoder().right().encoder()
        self.assertEqual(p.tempo, 1000, "maximum")

    def test_edit_swing(self):
        c = self.controller
        p = self.env.sequencer.model.project

        # initial swing
        self.assertEqual(p.swing, 50, "initial swing")

        # select swing
        c.right().right()

        # increase
        c.encoder().right().encoder().wait()
        self.assertEqual(p.swing, 51, "increase")

        # decrease
        c.encoder().left().encoder().wait()
        self.assertEqual(p.swing, 50, "decrease")

        # shift + increase
        c.encoder().down("shift").right().up("shift").encoder().wait()
        self.assertEqual(p.swing, 55, "shift + increase")

        # shift + decrease
        c.encoder().down("shift").left().up("shift").encoder().wait()
        self.assertEqual(p.swing, 50, "shift + decrease")

        # minimum
        p.swing = 50
        c.encoder().left().encoder()
        self.assertEqual(p.swing, 50, "minimum")

        # maximum
        p.swing = 75
        c.encoder().right().encoder()
        self.assertEqual(p.swing, 75, "maximum")

