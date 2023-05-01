import Pyro4

o = Pyro4.Proxy("PYRO:obj_8fe61ef982364ae495e50a1654ee8d16@localhost:57072")

print(o.notificar())
