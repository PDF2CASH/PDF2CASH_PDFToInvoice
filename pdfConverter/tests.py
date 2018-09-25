from django.test import TestCase

class TravisTest(TestCase):

    def test_must_broke(self):
        self.assertEqual(2,4)
