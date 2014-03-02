import Language.Atom.MSP430 as A

import Language.Copilot 
import Copilot.Compile.C99 as C
import qualified Prelude as P

setup = do
  watchdog <== Const (wdtPassword .|. wdtHold)
  port1Dir <== 0x0001

yes :: Stream Bool
yes = [True] ++ yes

spec :: Spec
spec = do
  trigger "led" yes []

main :: IO ()
main = do
  reify spec >>= C.compile C.defaultParams
  mspCompile "g2553" $ simpleProgram setup
