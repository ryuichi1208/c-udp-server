from unittest.mock import patch
import unittest

def get_data():
    return [4]

def process_data():
    return get_data()

class TestProcessData(unittest.TestCase):
    @patch('test_main.get_data')  # または '@patch('__main__.get_data')'
    def test_process_data(self, mock_get_data):
        mock_get_data.return_value = [1, 2, 3]
        assert process_data() == [1, 2, 3]
        mock_get_data.assert_called_once()
        assert process_data() != [1, 2, 3, 4]

if __name__ == '__main__':
    unittest.main()
