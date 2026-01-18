import pygame
import math
import random

# --- CONSTANTS & CONFIGURATION ---
WIDTH, HEIGHT = 800, 600
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (200, 50, 50)
GREEN = (50, 200, 50)
BLUE = (50, 50, 200)
ORANGE = (255, 165, 0) # Color for circles

# GA Settings
POPULATION_SIZE = 25
GENERATIONS = 20
MUTATION_RATE = 0.2

# --- 1. ENVIRONMENT SETUP ---
class Environment:
    def __init__(self):
        self.rects = []
        self.circles = [] # Format: (x, y, radius)
        self.start = (50, 50)
        self.goal = (700, 500)
        self.map_id = 1
        self.load_map_complex() 

    def load_map_simple(self):
        """Map 1: Simple Mix"""
        self.map_id = 1
        self.rects = []
        self.circles = []
        self.start = (50, 50)
        self.goal = (700, 500)
        
        # Walls
        self.rects.append(pygame.Rect(300, 100, 20, 400))
        self.rects.append(pygame.Rect(500, 200, 20, 300))
        
        # Circle Obstacles
        self.circles.append((400, 300, 40))
        self.circles.append((600, 100, 30))

    def load_map_complex(self):
        """Map 2: Zig-Zag Slalom (Redesigned & Solvable)"""
        self.map_id = 2
        self.rects = []
        self.circles = []
        # Center the start and goal for the slalom
        self.start = (50, 300)
        self.goal = (750, 300)
        
        # Wall 1: Down from top (Gap at bottom)
        self.rects.append(pygame.Rect(250, 0, 20, 400))
        
        # Wall 2: Up from bottom (Gap at top)
        self.rects.append(pygame.Rect(500, 200, 20, 400))
        
        # Difficult Circles placed in the gaps
        self.circles.append((260, 450, 30)) # Near first gap
        self.circles.append((490, 150, 30)) # Near second gap
        self.circles.append((650, 300, 40)) # Guarding goal

    def load_map_random(self):
        """Map 3: Procedural Random Generation"""
        self.map_id = 3
        self.rects = []
        self.circles = []
        self.start = (50, 50)
        self.goal = (700, 500)
        
        # Generate Random Rectangles
        for _ in range(8):
            w = random.randint(20, 150)
            h = random.randint(20, 150)
            x = random.randint(100, WIDTH - 100)
            y = random.randint(50, HEIGHT - 50)
            rect = pygame.Rect(x, y, w, h)
            
            # Don't cover start or goal
            start_safe = pygame.Rect(0,0,150,150)
            goal_safe = pygame.Rect(650,450,150,150)
            
            if not rect.colliderect(start_safe) and not rect.colliderect(goal_safe):
                self.rects.append(rect)

        # Generate Random Circles
        for _ in range(6):
            r = random.randint(20, 50)
            x = random.randint(100, WIDTH - 100)
            y = random.randint(50, HEIGHT - 50)
            
            # Simple distance check for start/goal safety
            d_start = math.hypot(x - self.start[0], y - self.start[1])
            d_goal = math.hypot(x - self.goal[0], y - self.goal[1])
            
            if d_start > 100 and d_goal > 100:
                self.circles.append((x, y, r))

    def draw(self, screen):
        pygame.draw.circle(screen, GREEN, self.goal, 15)
        # Draw Rects
        for r in self.rects:
            pygame.draw.rect(screen, BLACK, r)
        # Draw Circles
        for c in self.circles:
            pygame.draw.circle(screen, ORANGE, (c[0], c[1]), c[2])

# --- 2. ROBOT MODEL ---
class Robot:
    def __init__(self, start_pos):
        self.x, self.y = start_pos
        self.angle = 0  
        self.radius = 10
        self.sensor_range = 100
        # Indices: 0=L90, 1=L60, 2=L30, 3=Center, 4=R30, 5=R60, 6=R90
        self.sensors = [0.0] * 7 
        self.crashed = False

    def move(self, v, w):
        self.angle += w
        self.x += v * math.cos(self.angle)
        self.y += v * math.sin(self.angle)
        self.x = max(10, min(WIDTH-10, self.x))
        self.y = max(10, min(HEIGHT-10, self.y))

    def sense(self, env):
        sensor_angles = [-1.57, -1.0, -0.5, 0, 0.5, 1.0, 1.57] 
        readings = []
        for offset in sensor_angles:
            ray_angle = self.angle + offset
            dist = self.sensor_range
            found = False
            
            # Raycasting Step
            for r in range(0, self.sensor_range, 5):
                cx = self.x + r * math.cos(ray_angle)
                cy = self.y + r * math.sin(ray_angle)
                
                # 1. Screen Bounds
                if cx < 0 or cx > WIDTH or cy < 0 or cy > HEIGHT:
                    dist = r
                    found = True
                    break
                
                # 2. Rect Collision
                point_rect = pygame.Rect(cx, cy, 1, 1)
                for rect in env.rects:
                    if point_rect.colliderect(rect):
                        dist = r
                        found = True
                        break
                if found: break
                
                # 3. Circle Collision
                for circ in env.circles:
                    # circ is (x, y, r)
                    d_circ = math.hypot(cx - circ[0], cy - circ[1])
                    if d_circ < circ[2]:
                        dist = r
                        found = True
                        break
                if found: break

            readings.append(dist)
        self.sensors = readings

    def check_collision(self, env):
        # 1. Rect Collision
        robot_rect = pygame.Rect(self.x - self.radius, self.y - self.radius, 
                                 self.radius*2, self.radius*2)
        for rect in env.rects:
            if robot_rect.colliderect(rect):
                self.crashed = True
                return True
        
        # 2. Circle Collision
        for circ in env.circles:
            # Distance between centers < sum of radii
            d = math.hypot(self.x - circ[0], self.y - circ[1])
            if d < (self.radius + circ[2]):
                self.crashed = True
                return True
                
        return False

    def draw(self, screen):
        color = RED if self.crashed else BLUE
        pygame.draw.circle(screen, color, (int(self.x), int(self.y)), self.radius)
        sensor_angles = [-1.57, -1.0, -0.5, 0, 0.5, 1.0, 1.57] 
        for i, dist in enumerate(self.sensors):
            ray_angle = self.angle + sensor_angles[i]
            end_x = self.x + dist * math.cos(ray_angle)
            end_y = self.y + dist * math.sin(ray_angle)
            pygame.draw.line(screen, RED, (self.x, self.y), (end_x, end_y), 1)

# --- 3. HYBRID CONTROLLER ("THE BRAIN") ---
class HybridController:
    def __init__(self, genes):
        self.genes = genes 

    def compute(self, robot, goal):
        dx = goal[0] - robot.x
        dy = goal[1] - robot.y
        target_angle = math.atan2(dy, dx)
        heading_error = target_angle - robot.angle
        heading_error = (heading_error + math.pi) % (2 * math.pi) - math.pi

        # --- [FUZZIFICATION] ---
        # Converting Crisp Inputs (Distance) to Fuzzy-like range (0.0 to 1.0)
        norm_sensors = [s / robot.sensor_range for s in robot.sensors]
        
        # Extracting Genetic Parameters
        safety_thresh = self.genes[0]  # Evolved threshold for "Near"
        turn_speed = max(self.genes[1], 0.3)
        goal_weight = self.genes[2]
        
        # Defining Linguistic Variables (Simplified)
        front_danger = min(norm_sensors[2], norm_sensors[3], norm_sensors[4])
        side_danger = min(norm_sensors[0], norm_sensors[1], norm_sensors[5], norm_sensors[6])

        v = 3.0 
        w = 0.0

        # --- [INFERENCE / RULE BASE] ---
        
        # Rule 1: "Very Near" (Critical)
        if front_danger < 0.15: 
            v = -3.0  # Output: Reverse
            # Output: Turn away
            if sum(norm_sensors[:3]) > sum(norm_sensors[4:]):
                w = -0.6
            else:
                w = 0.6
        
        # Rule 2: "Near" (Caution)
        # Using the evolved 'safety_thresh' gene to decide what "Near" means
        elif front_danger < safety_thresh or side_danger < 0.3:
            v = 1.0 # Output: Slow Down
            
            # Logic to find clearer path
            left_clearance = sum(norm_sensors[:3]) 
            right_clearance = sum(norm_sensors[4:])
            if left_clearance > right_clearance:
                w = -turn_speed
            else:
                w = turn_speed
        
        # Rule 3: "Far" (Safe)
        else:
            # Output: Seek Goal
            w = heading_error * goal_weight
            w = max(min(w, turn_speed), -turn_speed)

        # --- [DEFUZZIFICATION] ---
        # (Implicitly handled by returning the crisp v, w values)
        return v, w

# --- 4. GA TRAINING ---
def run_simulation_headless(genes):
    env = Environment()
    robot = Robot(env.start)
    controller = HybridController(genes)
    
    max_steps = 1500
    total_steps = 0
    
    for _ in range(max_steps):
        total_steps += 1
        robot.sense(env)
        
        if robot.check_collision(env):
            return -500 + total_steps 
            
        v, w = controller.compute(robot, env.goal)
        robot.move(v, w)
        
        dist = math.hypot(robot.x - env.goal[0], robot.y - env.goal[1])
        if dist < 20:
            return 2000 - total_steps 
            
    return -100 - dist 

def genetic_training():
    print(f"--- STARTING TRAINING ({GENERATIONS} Gens) ---")
    population = []
    for _ in range(POPULATION_SIZE):
        gene = [
            random.uniform(0.3, 0.7),
            random.uniform(0.2, 0.6),
            random.uniform(0.5, 1.5)
        ]
        population.append(gene)

    best_overall_gene = population[0]
    
    for gen in range(GENERATIONS):
        scores = []
        for ind in population:
            fitness = run_simulation_headless(ind)
            scores.append((ind, fitness))
        
        scores.sort(key=lambda x: x[1], reverse=True)
        best_val = scores[0][1]
        
        status = "SUCCESS" if best_val > 0 else "FAIL"
        print(f"Gen {gen+1}: Best Fitness = {best_val:.0f} [{status}]")
        
        best_overall_gene = scores[0][0]
        
        survivors = [x[0] for x in scores[:POPULATION_SIZE//2]]
        next_gen = []
        while len(next_gen) < POPULATION_SIZE:
            p1 = random.choice(survivors)
            p2 = random.choice(survivors)
            child = []
            for i in range(len(p1)):
                val = p1[i] if random.random() < 0.5 else p2[i]
                if random.random() < MUTATION_RATE:
                    val += random.uniform(-0.1, 0.1)
                child.append(val)
            next_gen.append(child)
        population = next_gen

    print(f"--- OPTIMIZED GENES: {[round(x,2) for x in best_overall_gene]} ---")
    return best_overall_gene

# --- 5. MAIN VISUALIZATION ---
def main():
    best_genes = genetic_training()
    
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Final Robot: Rects, Circles & Random Maps")
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("Arial", 20)

    env = Environment()
    robot = Robot(env.start)
    controller = HybridController(best_genes)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_1:
                    env.load_map_simple()
                    robot = Robot(env.start)
                if event.key == pygame.K_2:
                    env.load_map_complex()
                    robot = Robot(env.start)
                if event.key == pygame.K_3:
                    env.load_map_random() # GENERATE RANDOM MAP
                    robot = Robot(env.start)
                if event.key == pygame.K_r:
                    robot = Robot(env.start)

        # Physics
        robot.sense(env)
        
        if robot.check_collision(env):
            # Bounce
            robot.x -= 15 * math.cos(robot.angle)
            robot.y -= 15 * math.sin(robot.angle)
            robot.angle += random.uniform(2.0, 4.0)
            robot.crashed = False
        
        v, w = controller.compute(robot, env.goal)
        robot.move(v, w)
        
        if math.hypot(robot.x - env.goal[0], robot.y - env.goal[1]) < 20:
            print("Goal Reached!")
            robot.x, robot.y = env.start
            robot.crashed = False

        # Drawing
        screen.fill(WHITE)
        env.draw(screen)
        robot.draw(screen)
        
        info = f"Map: {env.map_id} | Genes: {[round(g,2) for g in best_genes]}"
        screen.blit(font.render(info, True, BLACK), (10, 10))
        screen.blit(font.render("1: Simple | 2: Zig-Zag (Fixed) | 3: Random Map | R: Reset", True, BLUE), (10, 35))

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()

if __name__ == "__main__":
    main()
