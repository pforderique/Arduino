import keyboard
import time

# keyboard.add_abbreviation("@email", "fabrizzioorderique@gmail.com")
# keyboard.write("Python Programming is always fun!", delay=0.1)

class Controller():
    key_groups = [
        'abc', 'def', 'ghi', 'jkl', 'mno', 'pqr', 'stu', 'vwx', 'yz']

    def __init__(self) -> None:
        self.TYPE_TIMEOUT = 0.65
        self.typing = False
        self.timer = None
        self.keys_gen = None
        self.key = None
        self.prev_fsm_id = None
        
        for i, key_group in enumerate(self.key_groups, 1):
            setattr(self, f'key_group{i}', self._keys_fsm(key_group))

    def power(self):
        """Turn off computer? Turn off screen only?"""
        print("Not Implemented Yet.")

    def enter(self):
        keyboard.send('enter')

    def windows(self):
        keyboard.send('windows')

    def mute(self):
        return self._volume('mute')

    def volume_up(self):
        return self._volume('up')

    def volume_down(self):
        return self._volume('down')

    def _volume(self, command):
        keyboard.send(f'volume {command}')

    def up(self):
        keyboard.send('up')
    
    def down(self):
        keyboard.send('down')

    def left(self):
        keyboard.send('left')

    def right(self):
        keyboard.send('right')

    def tab(self):
        keyboard.send('tab')

    def backspace(self):
        keyboard.send('backspace')

    def space(self):
        keyboard.send(' ')

    def a(self):
        keyboard.send('a')

    def _keys_fsm(self, keys_list):
        def fsm():
            if self.timer is None:
                pass

            elif time.time() - self.timer > self.TYPE_TIMEOUT \
                or self.prev_fsm_id != id(fsm):
                self.typing = False
                self.timer = None
                self.keys_gen = None
                self.key = None
                return fsm()

            else:
                self.key = next(self.keys_gen)
                self.timer = time.time()

                time.sleep(0.2)
                keyboard.send('backspace')
                keyboard.send(self.key)

            if self.typing is False:
                self.typing = True
                self.timer = time.time()
                self.prev_fsm_id = id(fsm)

                self.keys_gen = cicular_gen(keys_list)
                self.key = next(self.keys_gen)
                keyboard.send(self.key)

        return fsm

    def close_window(self):
        keyboard.send('ctrl+w')

        
def cicular_gen(keyslist):
    for key in keyslist:
        yield key
    yield from cicular_gen(keyslist)