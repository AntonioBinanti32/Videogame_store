from flask import Flask, render_template, request, redirect, url_for, session
import requests

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Cambia con una chiave segreta sicura

# Configurazione delle rotte
@app.route('/')
def home():
    response = requests.get('http://backend_service/games')
    games = response.json()
    return render_template('home.html', games=games)

@app.route('/game/<int:game_id>')
def game(game_id):
    response = requests.get(f'http://backend_service/games/{game_id}')
    game = response.json()
    return render_template('game.html', game=game)

@app.route('/cart')
def cart():
    if 'cart' not in session:
        session['cart'] = []
    cart_items = session['cart']
    return render_template('cart.html', cart_items=cart_items)

@app.route('/add_to_cart/<int:game_id>')
def add_to_cart(game_id):
    if 'cart' not in session:
        session['cart'] = []
    session['cart'].append(game_id)
    return redirect(url_for('cart'))

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        credentials = request.form
        response = requests.post('http://backend_service/auth/login', json=credentials)
        if response.status_code == 200:
            session['user'] = response.json()['user']
            return redirect(url_for('home'))
        else:
            return 'Login Failed', 401
    return render_template('login.html')

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        user_data = request.form
        response = requests.post('http://backend_service/auth/signup', json=user_data)
        if response.status_code == 201:
            return redirect(url_for('login'))
        else:
            return 'Signup Failed', 400
    return render_template('signup.html')

@app.route('/reviews/<int:game_id>', methods=['GET', 'POST'])
def reviews(game_id):
    if request.method == 'POST':
        review_data = request.form
        response = requests.post(f'http://backend_service/games/{game_id}/reviews', json=review_data)
        return redirect(url_for('reviews', game_id=game_id))
    else:
        response = requests.get(f'http://backend_service/games/{game_id}/reviews')
        reviews = response.json()
        return render_template('reviews.html', reviews=reviews, game_id=game_id)

@app.route('/logout')
def logout():
    session.pop('user', None)
    return redirect(url_for('home'))

if __name__ == '__main__':
    app.run(debug=True)
